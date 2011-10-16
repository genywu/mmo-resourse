#include "StdAfx.h"
#include "SkillXml.h"
#include "CityGate.h"
#include "Other/WeaponEffectXml.h"
#include "Player.h"
#include "PetSystem/Pet.h"
#include "Monster.h"
#include "ClientRegion.h"
#include "Other/AudioList.h"
#include "Effect.h"
#include "../GameClient/GameControl.h"
#include "../../Public/Common/SkillRelated.h"
#include "../GameClient/Game.h"
#include "../../Input/KeyBoard.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSkillXml::CSkillXml()
{
	SetType(TYPE_SKILL);
	m_pLauncher = NULL;
	m_pDestShape = NULL;
	m_pMoveShape = NULL;

	m_lStep = -1;

	m_lSourType = 0;
	m_lDestX = 0;
	m_lDestY = 0;
	m_lSkillID = 0;
	m_lSkillLevel = 0;

	m_pSkillAI = NULL;

	m_fDir = 0.0f;

	//m_lStarEquipId = 0;
	//m_lStarEquipPlace = 0;

	m_lItemId =0;
	m_lItemNum = 0;
	m_lItemPlace = 0;

	m_lIntonateTime = 0;
	m_wIntonateMainAct = 0;

	m_lIntonateEffect = 0;
	m_lIntonateEffectIndex = 0;
	m_bIntonateEffectByEquip = false;
	m_bIntonateEffectCycle = 0;
	m_eIntonateEffectPosition = LOCATOR_FOOTCENTER;

	m_lIntonateSound = 0;
	m_bIntonateSoundByEquip = false;
	m_bIntonateSoundCycle = false;
	m_lReleaseSoundDest = 0;
	m_IntonateSoundChannel = 0;

	m_lReleaseEffect = 0;
	m_lReleaseEffectIndex = 0;
	m_bReleaseEffectByEquip = false;
	m_lReleaseEffectDest = 0;
	m_eReleaseEffectPosition = LOCATOR_FOOTCENTER;
	m_bReleaseEffectCycle = false;

	m_lReleaseSound = 0;
	m_bReleaseSoundByEquip = false;
	m_bReleaseSoundCycle = false;
	m_lReleaseSoundDest = 0;

	m_fShockRatio = 0.0f;
	m_lShockInterval = 0;
	m_lShockTimeLength = 0;

	m_lFlyEffectNum = 0;
	m_lFlyEffect = 0;
	m_lFlyEffectIndex = 0;
	m_bFlyEffectByEquip = false;
	m_eFlyEffectPosition = LOCATOR_FOOTCENTER;
	m_bFlyEffectCycle = false;
	m_lFlyEffectTurnX = 0;
	m_lFlyEffectTurnY = 0;

	m_lFlySound = 0;
	m_bFlySoundByEquip = false;
	m_bFlySoundCycle = false;
	m_eFlySoundPosition = LOCATOR_FOOTCENTER;

	m_lSummonLifeTime = 0;				// 召唤生命周期
	m_lRandomSummonX = 0;				// 随机召唤技能X轴范围
	m_lRandomSummonY = 0;				// 随机召唤技能Y轴范围
	m_lRandomSummonNum = 0;				// 随机召唤技能召唤物数
	m_lRandomSummonDelayTime = 0;		// 随即召唤技能召唤物的延迟播放时间

	m_lDelLocEffect = 0;
	m_lDelLocEffectIndex = 0;

	m_lCycleEffectDelayTime = 0;

	m_bIsLastActExCycle = FALSE;

	m_com = NULL;
}

CSkillXml::CSkillXml(SkillComponent* com):
m_com(com)
{
	SetType(TYPE_SKILL);
	m_pLauncher = NULL;
	m_pDestShape = NULL;
	m_pMoveShape = NULL;

	m_lStep = -1;

	m_lSourType = 0;
	m_lDestX = 0;
	m_lDestY = 0;
	m_lSkillID = 0;
	m_lSkillLevel = 0;

	m_pSkillAI = NULL;

	m_fDir = 0.0f;

	//m_lStarEquipId = 0;
	//m_lStarEquipPlace = 0;

	m_lItemId =0;
	m_lItemNum = 0;
	m_lItemPlace = 0;

	m_lIntonateTime = 0;
	m_wIntonateMainAct = 0;

	m_lIntonateEffect = 0;
	m_lIntonateEffectIndex = 0;
	m_bIntonateEffectByEquip = false;
	m_bIntonateEffectCycle = 0;
	m_eIntonateEffectPosition = LOCATOR_FOOTCENTER;

	m_lIntonateSound = 0;
	m_bIntonateSoundByEquip = false;
	m_bIntonateSoundCycle = false;
	m_lReleaseSoundDest = 0;
	m_IntonateSoundChannel = 0;

	m_lReleaseEffect = 0;
	m_lReleaseEffectIndex = 0;
	m_bReleaseEffectByEquip = false;
	m_lReleaseEffectDest = 0;
	m_eReleaseEffectPosition = LOCATOR_FOOTCENTER;
	m_bReleaseEffectCycle = false;

	m_lReleaseSound = 0;
	m_bReleaseSoundByEquip = false;
	m_bReleaseSoundCycle = false;
	m_lReleaseSoundDest = 0;

	m_fShockRatio = 0.0f;
	m_lShockInterval = 0;
	m_lShockTimeLength = 0;

	m_lFlyEffectNum = 0;
	m_lFlyEffect = 0;
	m_lFlyEffectIndex = 0;
	m_bFlyEffectByEquip = false;
	m_eFlyEffectPosition = LOCATOR_FOOTCENTER;
	m_bFlyEffectCycle = false;
	m_lFlyEffectTurnX = 0;
	m_lFlyEffectTurnY = 0;

	m_lFlySound = 0;
	m_bFlySoundByEquip = false;
	m_bFlySoundCycle = false;
	m_eFlySoundPosition = LOCATOR_FOOTCENTER;

	m_lSummonLifeTime = 0;				// 召唤生命周期
	m_lRandomSummonX = 0;				// 随机召唤技能X轴范围
	m_lRandomSummonY = 0;				// 随机召唤技能Y轴范围
	m_lRandomSummonNum = 0;				// 随机召唤技能召唤物数
	m_lRandomSummonDelayTime = 0;		// 随即召唤技能召唤物的延迟播放时间

	m_lDelLocEffect = 0;
	m_lDelLocEffectIndex = 0;

	m_lCycleEffectDelayTime = 0;

	m_bIsLastActExCycle = FALSE;
}
CSkillXml::~CSkillXml()
{
	// 删除所有对象的所有特效
	DelAllEffectListByStep(STEP_START);
	DelAllEffectListByStep(STEP_RUN);
	DelAllEffectListByStep(STEP_END);
	m_vecFlyEffect.clear();
	m_vecFlyEffectIndex.clear();
	m_vecFlyEffectPos.clear();
	m_vecFlyTime.clear();
	m_vecFlyDelay.clear();
	m_vecSummonEffect.clear();
	m_vecSummonSound.clear();
	m_vecDestMoveShape.clear();
	m_vecFlyEffectTurnX.clear();
	m_vecFlyEffectTurnY.clear();
	m_vecStarEquipId.clear();
	m_vecStarEquipPlace.clear();
	//////////////////////////////////////////////////////////////////////////
	// 测试召唤技能
	/*if(m_pSkillAI)
		m_pSkillAI->SetIsDeleted(true);*/
	//////////////////////////////////////////////////////////////////////////
	SAFE_DELETE(m_pSkillAI);
	SAFE_DELETE(m_com);
}
void CSkillXml::AI()
{
	CShape::AI();
	if(m_pSkillAI )
		m_pSkillAI->AI();
}
bool CSkillXml::Display()
{
	CShape::Display();
	if(m_pSkillAI)
		m_pSkillAI->Display();
	return true;
}
// 执行所有技能步骤
BOOL CSkillXml::ExecuteStep(int iWordKey, long lValue, const string& strValue)
{
	BOOL bChecked = TRUE;
	switch(iWordKey)
	{
	case SKILL_OBJECT				: (SkillObject(lValue))?bChecked = TRUE :bChecked = FALSE; break;
	case SKILL_FORCE_OBJECT			: (SkillForceObject(lValue))?bChecked = TRUE : bChecked = FALSE; break;
	case FIRS_TATT					: (FirstAtt(lValue))?bChecked = TRUE :bChecked = FALSE;; break;
	case SKILL_STAR_BUFF			: (SkillStarBuff(lValue))?bChecked = TRUE :bChecked = FALSE; break;
	case SKILL_STAR_STATE			: (SkillStarState(lValue))?bChecked = TRUE :bChecked = FALSE; break;
	case SKILL_STAR_EQUIP_ID		: SkillStarEquipId(lValue); break;
	case SKILL_STAR_EQUIP_PLACE		: SkillStarEquipPlace(lValue); break;
	case SKILL_STAR_EQUIP_USE		: (SkillStarEquipUse())?bChecked = TRUE :bChecked = FALSE; break;
	case SKILL_ITEM_ID				: SkillItemId(lValue); break;
	case SKILL_ITEM_NUM				: SkillItemNum(lValue); break;
	case SKILL_ITEM_PLACE			: SkillItemPlace(lValue); break;
	case SKILL_USE_ITEM				: (SkillUseItem())?bChecked = TRUE :bChecked = FALSE; break;
	case SKILL_HP					: (SkillHp(lValue))?bChecked = TRUE :bChecked = FALSE; break;
	case SKILL_MP					: (SkillMp(lValue))?bChecked = TRUE :bChecked = FALSE; break;
	case SKILL_ENERGY				: (SkillEnergy(lValue))? bChecked = TRUE :bChecked = FALSE; break;
	case INTONATE_LEAST_SPACE		: (IntonateLeastSpace(lValue))?bChecked = TRUE :bChecked = FALSE; break;
	case INTONATE_MOST_SPACE		: (IntonateMostSpace(lValue))?bChecked = TRUE :bChecked = FALSE; break;
	case FIRE_LEAST_SPACE			: (FireLeastSpace(lValue))?bChecked = TRUE :bChecked = FALSE; break;
	case FIRE_MOST_SPACE			: (FireMostSpace(lValue))?bChecked = TRUE :bChecked = FALSE; break;
	case INTONATE_ACT				: IntonateAct(lValue); break;
	case INTONATE_EFFECT			: IntonateEffect(lValue); break;
	case INTONATE_EFFECT_INDEX		: IntonateEffectIndex(lValue); break;
	case INTONATE_EFFECT_BY_EQUIP	: IntonateEffectByEquip(lValue); break;
	case INTONATE_EFFECT_POSITION	: IntonateEffectPosition(lValue); break;
	case INTONATE_EFFECT_CYCLE		: IntonateEffectCycle(lValue); break;
	case INTONATE_PLAY				: IntonatePlay(); break;
	case INTONATE_SOUND				: IntonateSound(lValue); break;
	case INTONATE_SOUND_BY_EQUIP	: IntonateSoundByEquip(lValue); break;
	case INTONATE_SOUND_CYCLE		: IntonateSoundCycle(lValue); break;
	case INTONATE_SOUND_PLAY		: IntonateSoundPlay(); break;
	case INTONATE_BREAK				: IntonateBreak(lValue); break;
	case RELEASE_ACT				: ReleaseAct(lValue); break;
	case RELEASE_EFFECT				: ReleaseEffect(lValue); break;
	case RELEASE_EFFECT_INDEX		: ReleaseEffectIndex(lValue); break;
	case RELEASE_EFECT_BY_EQUIP		: ReleaseEffectByEquip(lValue); break;
	case RELEASE_EFFECT_DEST		: ReleaseEffectDest(lValue); break;
	case RELEASE_EFFECT_POSITION	: ReleaseEffectPosition(lValue); break;
	case RELEASE_EFFECT_CYCLE		: ReleaseEffectCycle(lValue); break;
	case RELEASE_PLAY				: ReleasePlay(); break;
	case RELEASE_SOUND				: ReleaseSound(lValue); break;
	case RELEASE_SOUND_BY_EQUIP		: ReleaseSoundByEquip(lValue); break;
	case RELEASE_SOUND_CYCLE		: ReleaseSoundCycle(lValue); break;
	case RELEASE_SOUND_DEST			: ReleaseSoundDest(lValue); break;
	case RELEASE_SOUND_PLAY			: ReleaseSoundPlay();break;
	case SHOCK_RATIO				: ShockRatio(lValue); break;
	case SHOCK_INTERVAL				: ShockInterval(lValue); break;
	case SHOCK_TIME_LENGTH			: ShockTimeLength(lValue); break;
	case SHOCK_PLAY					: ShockPlay(); break;
	case FLY_EFFECT					: FlyEffect(lValue); break;
	case FLY_EFFECT_INDEX			: FlyEffectIndex(lValue); break;
	case FLY_EFFECT_BY_EQUIP		: FlyEffectByEquip(lValue); break;
	case FLY_EFFECT_POSITION		: FlyEffectPosition(lValue); break;
	case FLY_EFFECT_CYCLE			: FlyEffectCycle(lValue); break;
	case FLY_EFFECT_TURNX			: FlyEffectTurnX(lValue); break;
	case FLY_EFFECT_TURNY			: FlyEffectTurnY(lValue); break;
	case FLY_PLAY					: FlyPlay(); break;
	case FLY_SOUND					: FlySound(lValue); break;
	case FLY_SOUND_BY_EQUIP			: FlySoundByEquip(lValue); break;
	case FLY_SOUND_POSITION			: FlySoundPosition(lValue); break;
	case FLY_SOUND_CYCLE			: FlySoundCycle(lValue); break;
	case FLY_SOUND_PLAY				: FlySoundPlay(); break;
	case FLY_TIME					: FlyTime(lValue); break;
	case FLY_DELAY					: FlyDelay(lValue); break;
	case DEL_LOC_EFFECT				: DelLocEffect(lValue); break;
	case DEL_LOC_EFFECT_INDEX		: DelLocEffectIndex(lValue); break;
	case START_DEL_EFFECT			: StartDelEffect(lValue); break;
	case SUMMON_EFFECT				: SummonEffect(lValue); break;
	case SUMMON_SOUND				: SummonSound(lValue); break;
	case SUMMON_LIFETIME			: SummonLifeTime(lValue); break;
	case RANDOM_SUMMONX				: RandomSummonX(lValue); break;
	case RANDOM_SUMMONY				: RandomSummonY(lValue); break;
	case RANDOM_SUMMON_NUM			: RandomSummonNum(lValue); break;
	case INTONATE_TIME				: IntonateTime(lValue);	break;					// 吟唱时间
	case FLY_EFFECT_NUM				: FlyEffectNum(lValue);	break;					// 飞行特效个数
	case INTONATE_MAINACT			: IntonateMainAct((WORD)lValue); break;				// 吟唱阶段需要循环播放的主要动作(74)
	case SUMMON_DELAYTIMA			: RandomSummonDelayTime(lValue); break;			// 随机召唤物延迟播放时间（75）
	case WEAPONEFFECT				: WeaponEffect(lValue); break;					// 释放技能时，是否播放武器特效的标识（76）
	case CHANGEPROPERTY				: ChangeProperty(lValue,strValue); break;		// 设置释放者的某个属性(77)
	case ACTTIME					: SetActTime((DWORD)lValue); break;				// 设置当前动作的规定时间(78)
	case RELEASE_ACTEX				: ReleaseActEx(lValue); break;					// 设置释放阶段的后续动作（79）
	case DELAY_CYCELEFFECT_TIME		: SetCycleEffectDelayTime(lValue); break;		// 设置循环特效延迟删除时间（80）
	case LAST_ACTEX_ISCYCEL			: SetActExIsCycle(lValue>0?TRUE:FALSE);	break;	// 设置最后一个释放阶段的后继动作是否循环播放(81)
	default:break;
	}
	return bChecked;
}
// 判断哪些职业可以使用当前技能
BOOL CSkillXml::SkillOccType(CSkillListXml::tagSkill *pSkill, CMoveShape *sShape)
{
	if (pSkill->vecUseByOcc.size()== 0)	// 如果没有职业判定就直接返回
		return TRUE;
	int iCounts = 0;
	vector<CSkillListXml::tagSkillOcc>::iterator it;
	for (it = pSkill->vecUseByOcc.begin(); it!=pSkill->vecUseByOcc.end(); it++)
	{
		if(sShape->GetType() == TYPE_PLAYER)
		{
			if(((CPlayer*)sShape)->GetOccupation() == it->dwOccId)
			iCounts++;
		}
		else if(sShape->GetType() == TYPE_PET)
		{
			if(((CPet*)sShape)->GetPetType() == it->dwOccId)
				iCounts++;
		}
	}
	if (iCounts>0)
		return TRUE;
	else
		return FALSE;

}
//////////////////////////////////////////////////////////////////////////
// 执行发送技能消息步骤
//////////////////////////////////////////////////////////////////////////
bool CSkillXml::StepSend(DWORD dwID,DWORD dwLevel,
						 CMoveShape *sShape, CMoveShape *dShape, 
						 long dX, long dY)
{
	int iCount = 0;	// 条件计数器
	bool bFlag = false;
	m_pLauncher = sShape;
	m_pDestShape = dShape;
	m_lDestX = dX;
	m_lDestY = dY;
	m_lSkillID = dwID;
	m_lSkillLevel = dwLevel;
	CSkillListXml::tagSkill *pSkill = CSkillListXml::GetProperty(dwID);
	if(NULL == pSkill)
		return FALSE;

	if(SkillOccType(pSkill, sShape) == FALSE)	// 判断职业属性
		return FALSE;
	// 锁定目标技能，目标死亡，且不能对死亡目标发送技能
	if(dShape && dShape->IsDied() && pSkill->bLockTarget && !pSkill->bIsUseToDiedObj)
	{
		/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
															CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_OBJDIED),
															D3DCOLOR_ARGB(255,255,0,0));*/
		return FALSE;
	}
	DWORD dwlev = pSkill->vectorLevels.size() >= dwLevel? dwLevel - 1:(pSkill->vectorLevels.size() - 1);
	vector<CSkillListXml::tagLevelValue> vectSend = pSkill->vectorLevels[dwlev].vecStepSend;
	vector<CSkillListXml::tagLevelValue>::iterator it;
	for(it = vectSend.begin(); it!= vectSend.end(); it++)
	{
		if(strcmp(it->strKeyWord.c_str(),"SkillObject")==0 || strcmp(it->strKeyWord.c_str(),"SkillForceObject")==0)
		{
			bFlag = true;
			// 攻击目标条件还没达成，继续查找下一个条件
			if(iCount == 0)
			{
				long lID				= it->dwID;
				long lValue		= it->dwValue;
				string strValue	= it->strValue;
				if(ExecuteStep(lID, lValue, strValue))	// 如果技能目标条件成立计数加1
					iCount ++;
			}
			// 如果攻击对象条件达成
			else
				continue;
		}
		else 
		{
			if(ExecuteStep(it->dwID, it->dwValue, it->strValue))	// 剩下的条件只要一个不成立就返回
				continue;
			else
				return FALSE;
		}
		
	}
	if(iCount>0 && bFlag)							// 如果计数器大于0表示目标至少有一个符合条件
		return TRUE;
	// 无使用对象设定
	else if(!bFlag)
		return TRUE;
	else
	{
		//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
		//													CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_OBJERROR),
		//													D3DCOLOR_ARGB(255,255,0,0));
		return FALSE;
	}
}
// 检验技能施放对象
BOOL CSkillXml::SkillObject(long l)
{
	switch(l)
	{
	case 1:		// 本人
		if(m_pDestShape && m_pDestShape == m_pLauncher)
			return TRUE;
		break;
	case 2:		// 敌人 或者 敌宠
		if(m_pDestShape && (m_pDestShape->GetType() == TYPE_PLAYER || m_pDestShape->GetType() == TYPE_PET) && m_pDestShape != m_pLauncher)
		{
			CPlayer* pDestPlayer = ConvertFromPetOrPlayer(m_pDestShape);
			if(pDestPlayer == NULL)		return false;
			CPlayer* pLauncher =  ConvertFromPetOrPlayer(m_pLauncher);
			if(pLauncher == NULL)	return false;

			if(pLauncher && (pLauncher->IsCityWarEnemyFaction(pDestPlayer->GetFactionID()) == true ||
				pLauncher->IsEnemyFaction(pDestPlayer->GetFactionID()) == true))
			{
				return true;
			}
		}
		break;
	case 3:		// 怪物
		if(m_pDestShape && m_pDestShape->GetType() == TYPE_MONSTER)
		{
			CMonster* pDestMonster = dynamic_cast<CMonster *>(m_pDestShape);
			if(!pDestMonster->IsDied() && pDestMonster->GeteNpcType() != NT_Normal)
				return TRUE;
		}
		break;

    // 宠物代码注释 Bugs 2008-9-12
    //case 4:		// 宠物
	//	if(m_pDestShape && m_pDestShape->GetType() == TYPE_MONSTER)
	//	{
	//		CMonster* pDestPet = dynamic_cast<CMonster*>(m_pDestShape);
	//		if(pDestPet->GetPetInfo()/* && pDestPet->IsDied() != true*/)
	//			return TRUE;
	//	}
	//	break;
	case 5:		// NPC
		if(m_pDestShape && m_pDestShape->GetType() == TYPE_MONSTER)
		{
			CMonster* pDestNpc = dynamic_cast<CMonster*>(m_pDestShape);
			if(pDestNpc->GeteNpcType() == NT_Normal/* && pDestNpc->IsDied()!= true*/)
				return TRUE;
		}
		break;
	case 6:		// 特殊怪物S_MONSTER
		if(m_pDestShape && m_pDestShape->GetType() == TYPE_MONSTER)
		{
			CMonster *pDestMonster = dynamic_cast<CMonster*>(m_pDestShape);
			if(pDestMonster->GeteNpcType() == NT_Monster/* && pDestMonster->IsDied()!=true*/)
				return TRUE;
		}
		break;
	case 7:		// 场景区域
		//if(m_pDestShape == NULL && m_lDestX>=0 && m_lDestY>=0)
		return TRUE;
	case 8:		// 海盗标记玩家
		if(m_pDestShape && m_pDestShape->GetType() == TYPE_PLAYER && m_pDestShape != m_pLauncher)
		{
			CPlayer* pDestPlayer = dynamic_cast<CPlayer *>(m_pDestShape);
			if(pDestPlayer->GetPVPValue() > 0)
				return TRUE;
		}
		break;
	case 9:		// 城门
		if(m_pDestShape && m_pDestShape->GetType() == TYPE_CITYGATE)
		{
			CCityGate* pDestCityGate = dynamic_cast<CCityGate *>(m_pDestShape);
			if(pDestCityGate->IsDied()!=true&&pDestCityGate->GetSwitch()!=1)  //城门没有毁坏或没有无敌
			    return TRUE;
		}
		break;
	case 31:	// 本国正常玩家或者宠物
		if(m_pDestShape && (m_pDestShape->GetType() == TYPE_PLAYER || m_pDestShape->GetType() == TYPE_PET) && m_pDestShape!= m_pLauncher)
		{
			CPlayer* pDestPlayer = ConvertFromPetOrPlayer(m_pDestShape);
			if(pDestPlayer == NULL)		return false;
			CPlayer* pLauncher =  ConvertFromPetOrPlayer(m_pLauncher);
			if(pLauncher == NULL)	return false;

			if(pLauncher && (pLauncher->IsCityWarEnemyFaction(pDestPlayer->GetFactionID()) == false||
					pLauncher->IsEnemyFaction(pDestPlayer->GetFactionID()) == false)/* && pDestPlayer->IsDied() != true*/
					&& pDestPlayer->GetPkValue() == 0)
					return TRUE;
		}
		break;
	case 32:	// 本国紫名玩家 或者宠物
		if(m_pDestShape && (m_pDestShape->GetType() == TYPE_PLAYER || m_pDestShape->GetType() == TYPE_PET) && m_pDestShape!= m_pLauncher)
		{
			CPlayer* pDestPlayer = ConvertFromPetOrPlayer(m_pDestShape);
			if(pDestPlayer == NULL)		return false;
			CPlayer* pLauncher =  ConvertFromPetOrPlayer(m_pLauncher);
			if(pLauncher == NULL)	return false;

			if((pLauncher->IsCityWarEnemyFaction(pDestPlayer->GetFactionID()) == false||
				pLauncher->IsEnemyFaction(pDestPlayer->GetFactionID()) == false)/* && pDestPlayer->IsDied() != true*/
				&& pDestPlayer->GetPkValue() > 0)
				return TRUE;
		}
		break;
	case 33:	// 本国红名玩家 或者宠物
		if(m_pDestShape && (m_pDestShape->GetType() == TYPE_PLAYER || m_pDestShape->GetType() == TYPE_PET) && m_pDestShape!= m_pLauncher)
		{
			CPlayer* pDestPlayer = ConvertFromPetOrPlayer(m_pDestShape);
			if(pDestPlayer == NULL)		return false;
			CPlayer* pLauncher =  ConvertFromPetOrPlayer(m_pLauncher);
			if(pLauncher == NULL)	return false;

			if((pLauncher->IsCityWarEnemyFaction(pDestPlayer->GetFactionID()) == false||
				pLauncher->IsEnemyFaction(pDestPlayer->GetFactionID()) == false)/* && pDestPlayer->IsDied() != true*/
				&& pDestPlayer->GetPkValue() > 1000)
				return TRUE;
		}
		break;
	}
	return FALSE;
}
// 检验技能强制施放对象
BOOL CSkillXml::SkillForceObject(long l)
{
	// 按下Ctrl强制攻击
	if(!GetKeyboard()->IsKeyDown(DIK_LCONTROL) && !GetKeyboard()->IsKeyDown(DIK_RCONTROL))
		return FALSE;
	switch(l)
	{
	case 1:		// 本人
		if(m_pDestShape && m_pDestShape == m_pLauncher)
			return TRUE;
		break;
	case 2:		// 敌人 或者 宠物
		if(m_pDestShape && (m_pDestShape->GetType() == TYPE_PLAYER || m_pDestShape->GetType() == TYPE_PET) && m_pDestShape != m_pLauncher)
		{
			CPlayer* pDestPlayer = ConvertFromPetOrPlayer(m_pDestShape);
			if(pDestPlayer == NULL)		return false;
			CPlayer* pLauncher =  ConvertFromPetOrPlayer(m_pLauncher);
			if(pLauncher == NULL)	return false;
			if(pLauncher->IsCityWarEnemyFaction(pDestPlayer->GetFactionID()) == true||
				pLauncher->IsEnemyFaction(pDestPlayer->GetFactionID()) == true/* && pDestPlayer->IsDied() != true*/)
				return TRUE;
		}
		break;
	case 3:		// 怪物
		if(m_pDestShape && m_pDestShape->GetType() == TYPE_MONSTER)
		{
			CMonster* pDestMonster = dynamic_cast<CMonster *>(m_pDestShape);
			if(!pDestMonster->IsDied() && pDestMonster->GeteNpcType() != NT_Normal)
				return TRUE;
		}
		break;
	//case 4:		// 宠物
	//	if(m_pDestShape && m_pDestShape->GetType() == TYPE_MONSTER)
	//	{
	//		CMonster* pDestPet = dynamic_cast<CMonster*>(m_pDestShape);
	//		if(pDestPet->GetPetInfo() && pDestPet->IsDied() != true)
	//			return TRUE;
	//	}
	//	break;
	case 5:		// NPC
		if(m_pDestShape && m_pDestShape->GetType() == TYPE_MONSTER )
		{
			CMonster* pDestNpc = dynamic_cast<CMonster*>(m_pDestShape);
			if(pDestNpc->GeteNpcType() == NT_Normal/* && pDestNpc->IsDied()!= true*/)
				return TRUE;
		}
		break;
	case 6:		// 特殊怪物S_MONSTER
		if(m_pDestShape && m_pDestShape->GetType() == TYPE_MONSTER)
		{
			CMonster *pDestMonster = dynamic_cast<CMonster*>(m_pDestShape);
			if(pDestMonster->GeteNpcType() == NT_Monster/* && pDestMonster->IsDied()!=true*/)
				return TRUE;
		}
		break;
	case 7:		// 场景区域
		//if(m_pDestShape == NULL && m_lDestX>=0 && m_lDestY>=0)
		return TRUE;
	case 8:		// 海盗标记玩家
		if(m_pDestShape && m_pDestShape->GetType() == TYPE_PLAYER && m_pDestShape != m_pLauncher)
		{
			CPlayer* pDestPlayer = dynamic_cast<CPlayer *>(m_pDestShape);
			if(pDestPlayer->GetPVPValue() > 0)
				return TRUE;
		}
		break;
	case 9:		// 城门
		if(m_pDestShape && m_pDestShape->GetType() == TYPE_CITYGATE)
		{
			CCityGate* pDestCityGate = dynamic_cast<CCityGate *>(m_pDestShape);
			if(pDestCityGate->IsDied()!=true&&pDestCityGate->GetSwitch()!=1)  //城门没有毁坏或没有无敌
			    return TRUE;
		}
		break;
	case 31:	// 本国正常玩家 或者 宠物
		if(m_pDestShape && (m_pDestShape->GetType() == TYPE_PLAYER || m_pDestShape->GetType() == TYPE_PET) && m_pDestShape!= m_pLauncher)
		{
			CPlayer* pDestPlayer = ConvertFromPetOrPlayer(m_pDestShape);
			if(pDestPlayer == NULL)		return false;
			CPlayer* pLauncher =  ConvertFromPetOrPlayer(m_pLauncher);
			if(pLauncher == NULL)	return false;

			if((pLauncher->IsCityWarEnemyFaction(pDestPlayer->GetFactionID()) == false||
				pLauncher->IsEnemyFaction(pDestPlayer->GetFactionID()) == false)/* && pDestPlayer->IsDied() != true*/
				&& pDestPlayer->GetPkValue() == 0)
				return TRUE;
		}
		break;
	case 32:	// 本国紫名玩家
		if(m_pDestShape && (m_pDestShape->GetType() == TYPE_PLAYER || m_pDestShape->GetType() == TYPE_PET) && m_pDestShape!= m_pLauncher)
		{
			CPlayer* pDestPlayer = ConvertFromPetOrPlayer(m_pDestShape);
			if(pDestPlayer == NULL)		return false;
			CPlayer* pLauncher =  ConvertFromPetOrPlayer(m_pLauncher);
			if(pLauncher == NULL)	return false;

			if((pLauncher->IsCityWarEnemyFaction(pDestPlayer->GetFactionID()) == false||
				pLauncher->IsEnemyFaction(pDestPlayer->GetFactionID()) == false)/* && pDestPlayer->IsDied() != true*/
				&& pDestPlayer->GetPkValue() > 0)
				return TRUE;
		}
		break;
	case 33:	// 本国红名玩家
		if(m_pDestShape && (m_pDestShape->GetType() == TYPE_PLAYER || m_pDestShape->GetType() == TYPE_PET) && m_pDestShape!= m_pLauncher)
		{
			CPlayer* pDestPlayer = ConvertFromPetOrPlayer(m_pDestShape);
			if(pDestPlayer == NULL)		return false;
			CPlayer* pLauncher =  ConvertFromPetOrPlayer(m_pLauncher);
			if(pLauncher == NULL)	return false;

			if((pLauncher->IsCityWarEnemyFaction(pDestPlayer->GetFactionID()) == false||
				pLauncher->IsEnemyFaction(pDestPlayer->GetFactionID()) == false)/* && pDestPlayer->IsDied() != true*/
				&& pDestPlayer->GetPkValue() > 1000)
				return TRUE;
		}
		break;
	}
	return FALSE;
}
// 是否可以对正常玩家或者宠物使用技能
BOOL CSkillXml::FirstAtt(long l)
{
	bool bAtt = (l==0)? false:true;
	bool bIsNormal = false;
	if(m_pDestShape && (m_pDestShape->GetType() == TYPE_PLAYER || m_pDestShape->GetType() == TYPE_PET) && m_pDestShape!= m_pLauncher)
	{
		CPlayer* pDestPlayer = ConvertFromPetOrPlayer(m_pDestShape);
		if(pDestPlayer == NULL)		return false;
		CPlayer* pLauncher =  ConvertFromPetOrPlayer(m_pLauncher);
		if(pLauncher == NULL)	return false;

		if((pLauncher->IsCityWarEnemyFaction(pDestPlayer->GetFactionID()) == false||
			pLauncher->IsEnemyFaction(pDestPlayer->GetFactionID()) == false) &&
			/*pDestPlayer->IsDied() != true && */pDestPlayer->GetPkValue() == 0)
			bIsNormal = true;
		if(bIsNormal && bAtt)
			return TRUE;
		else
			return FALSE;
	}
	return TRUE;
}
// 判断要施放技能所需的buff
BOOL CSkillXml::SkillStarBuff(long l)
{
	if(m_pLauncher->IsHaveStateEx(l))
		return TRUE;
	else
		return FALSE;
}
// 判断施放技能时玩家必须处于何种游戏状态
BOOL CSkillXml::SkillStarState(long l)
{
	if(m_pLauncher->GetState() == l)
		return TRUE;
	else
		return FALSE;
}
// 检测要施放技能所需装备的id
void CSkillXml::SkillStarEquipId(long l)
{
	m_vecStarEquipId.push_back(l);
}
// 检测要施放技能所需装备需放在何处
void CSkillXml::SkillStarEquipPlace(long l)
{
	 m_vecStarEquipPlace.push_back(l);
}
// 要施放技能必须的装备条件
BOOL CSkillXml::SkillStarEquipUse()
{
	BOOL bFlag = FALSE;
	CPlayer *pPlayer = GetGame()->GetMainPlayer();
	if(m_vecStarEquipId.size() == m_vecStarEquipPlace.size() && pPlayer)
	{
		for (size_t i = 0; i<m_vecStarEquipId.size(); ++i)
		{
			//// 盾牌
			//if(m_vecStarEquipId[i] == -1)
			//{
			//	CGoods *pGoods = pPlayer->GetEquip(CPlayer::EQUIP_WEAPON2);
			//	if(pGoods)
			//	{
			//		CGoodsList::tagBaseProperty* pBPro = pGoods->GetProperty();
			//		if(pBPro && pBPro->dwType == GT_WEAPON2)
			//		{
			//			bFlag = TRUE;
			//			break;
			//		}
			//	}
			//}
			//else 
            if(m_pLauncher->GetType() == TYPE_PLAYER && 
                ((CPlayer*)m_pLauncher)->FindAppointGoods(1,m_vecStarEquipId[i],0,m_vecStarEquipPlace[i]))
			{
				bFlag = TRUE;
				break;
			}
			else if(m_pLauncher->GetType() == TYPE_PET)		//宠物不判断
			{
				bFlag = TRUE;
				break;
			}
		}
	}
	//if(!bFlag)
	//	GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
	//														CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_ARM),
	//														D3DCOLOR_ARGB(255,255,0,0));
	return bFlag;
}
// 检测施放技能所需的道具
void CSkillXml::SkillItemId(long l)
{
	m_lItemId = l;
}
// 检测施放技能所需道具的数量
void CSkillXml::SkillItemNum(long l)
{
	m_lItemNum = l;
}
// 检测施放技能所需道具放置何处
void CSkillXml::SkillItemPlace(long l)
{
	m_lItemPlace = l;
}
// 判断施放技能所需道具条件
BOOL CSkillXml::SkillUseItem()
{
	BOOL bFlag =false;
	if(m_pLauncher->GetType() == TYPE_PLAYER)
	{
		bFlag = ((CPlayer*)m_pLauncher)->FindAppointGoods(2,m_lItemId,m_lItemNum,m_lItemPlace);
		//if(!bFlag)
		//	GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
		//	CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_GOODS),
		//	D3DCOLOR_ARGB(255,255,0,0));
	}
	else if(m_pLauncher->GetType() == TYPE_PET)		//宠物暂不判断
		bFlag = true;
	return bFlag;
}
// 判断施放技能所需mp
BOOL CSkillXml::SkillHp(long l)
{
	if( (long)m_pLauncher->GetHp() >= l)
		return TRUE;
	else
	{
		//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
		//													CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_HP),
		//													D3DCOLOR_ARGB(255,255,0,0));
		return FALSE;
	}
}
// 判断施放技能所需能量
BOOL CSkillXml::SkillMp(long l)
{
	long lMp = 0;
	if(m_pLauncher->GetType() == TYPE_PLAYER)
		lMp = ((CPlayer*)m_pLauncher)->GetMp();
	else if(m_pLauncher->GetType() == TYPE_PLAYER)
		lMp = ((CPet*)m_pLauncher)->GetMp();
	
	if(lMp >= l)
		return TRUE;
	else
	{
		//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
		//													CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_MP),
		//													D3DCOLOR_ARGB(255,255,0,0));
		return FALSE;
	}
}
// 判断施放技能所需能量
BOOL CSkillXml::SkillEnergy(long l)
{
	long lEnergy = 0;
	if(m_pLauncher->GetType() == TYPE_PLAYER)
		lEnergy = ((CPlayer*)m_pLauncher)->GetEnergy();
	else if(m_pLauncher->GetType() == TYPE_PLAYER)
		lEnergy = 0;
	if(lEnergy>= l)
		return TRUE;
	else
	{
		//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
		//													CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_NP),
		//													D3DCOLOR_ARGB(255,255,0,0));
		return FALSE;
	}
}
// 判断吟唱最小施放距离
BOOL CSkillXml::IntonateLeastSpace(long l)
{
	if(m_pDestShape && m_pLauncher->Distance(m_pDestShape) >=l)
		return TRUE;
	else if(m_pDestShape == NULL && m_pLauncher->RealDistance(m_lDestX,m_lDestY) >=l)
		return TRUE;
	else
	{
		//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
		//													CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_INT_MINDEST),
		//													D3DCOLOR_ARGB(255,255,0,0));
		return FALSE;
	}
}
// 判断吟唱最大施放距离
BOOL CSkillXml::IntonateMostSpace(long l)
{
	if(m_pDestShape && m_pLauncher->Distance(m_pDestShape) <= l)
		return TRUE;
	else if(m_pDestShape == NULL && m_pLauncher->RealDistance(m_lDestX,m_lDestY) <= l)
		return TRUE;
	else
	{
		//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
		//													CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_INT_MAXDEST),
		//													D3DCOLOR_ARGB(255,255,0,0));
		return FALSE;
	}
}
// 判断施放最小施放距离
BOOL CSkillXml::FireLeastSpace(long l)
{
	long lDistance = 0;
	if(m_pDestShape )
		lDistance = m_pLauncher->Distance(m_pDestShape);
	else if(m_pDestShape == NULL )
		lDistance = m_pLauncher->RealDistance(m_lDestX,m_lDestY); 
	if(lDistance >= l)
		return TRUE;
	else
	{
		//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
		//													CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_MINDEST),
		//													D3DCOLOR_ARGB(255,255,0,0));
		return FALSE;
	}
}
// 判断施放最大施放距离
BOOL CSkillXml::FireMostSpace(long l)
{
	long lDistance = 0;
	if(m_pDestShape )
		lDistance = m_pLauncher->Distance(m_pDestShape);

	else if(m_pDestShape == NULL )
		lDistance = m_pLauncher->RealDistance(m_lDestX,m_lDestY); 
	if(lDistance <= l)
		return TRUE;
	else
	{
		//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
		//													CStringReading::LoadText(eIDS_SKILL,eIDS_USE_UNSUCCESS_MAXDEST),
		//													D3DCOLOR_ARGB(255,255,0,0));
		return FALSE;
	}
}


//////////////////////////////////////////////////////////////////////////
// 执行技能开始步骤
//////////////////////////////////////////////////////////////////////////
bool CSkillXml::StepBegin(long lID, long lLevel, 
						  long sType, const CGUID& sID,
						  long lDestX, long lDestY, float fDir,
						  vector<long> vType,
						  vector<CGUID> vID)
{
	CPlayer *pPlayer = GetGame()->GetMainPlayer();
	m_lStep = 0;
	m_pMoveShape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(sType,sID));
	vector<long>::iterator itShape;
	int index = 0;
	if(m_vecDestMoveShape.size()>0)
		m_vecDestMoveShape.clear();
	for(itShape = vType.begin(); itShape!=vType.end(); itShape++)
	{
		CMoveShape* moveshape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject((*itShape),vID[index]));
		if(moveshape)
		{		
			m_vecDestMoveShape.push_back(moveshape);
			// 如果被攻击者属于任务保护怪
			CMonster *pMonster = dynamic_cast<CMonster *>(moveshape);
			if(m_pMoveShape == pPlayer && pMonster && 
				pMonster->GetCreatorType() != 0 && pMonster->GetCreatorID() != CGUID::GUID_INVALID)
			{
				// 创建者是monster，判断是否是守卫
				if(pMonster->GetCreatorType() == TYPE_MONSTER)
				{
					CMonster* pCreater = dynamic_cast<CMonster*>(GetGame()->GetRegion()->FindChildObject(pMonster->GetCreatorType(),pMonster->GetCreatorID()));
					if(pCreater && pCreater->GeteNpcType() == NT_Guard)
					{
						/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
							CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_ATTACK_QUESTOBJ),D3DCOLOR_ARGB(255,255,0,0),
							D3DCOLOR_ARGB(255,255,0,0));*/
					}
				}
				// 创建者与本玩家没关联，输出提示信息
				else if((pMonster->GetCreatorType() == TYPE_PLAYER && pMonster->GetCreatorID() != pPlayer->GetExID())
					|| (pMonster->GetCreatorType() == TYPE_TEAM && pMonster->GetCreatorID() != pPlayer->GetTeamID()))
				{
					/*GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,
																		CStringReading::LoadText(eIDS_SKILL,eIDS_ERROR_ATTACK_QUESTOBJ),D3DCOLOR_ARGB(255,255,0,0),
																		D3DCOLOR_ARGB(255,255,0,0));*/
				}
			}
		}
		index++;
	}
	/*if(m_vecDestMoveShape.size() == 0)
		return FALSE;*/
	m_lSourType = sType;
	m_SourID = sID;
	m_lDestType = (vType.size()>0)?vType[0]:0;
	m_DestID = (vID.size()>0)?vID[0]:CGUID::GUID_INVALID;
	m_lDestX = lDestX;
	m_lDestY = lDestY;
	m_fDir = fDir;
	m_lSkillID = lID;
	m_lSkillLevel = lLevel;

	//SetPosXY(m_pMoveShape->GetPosX(), m_pMoveShape->GetPosY());
	CSkillListXml::tagSkill *pSkill = CSkillListXml::GetProperty(lID);
	if(pSkill==NULL)
		return FALSE;
	DWORD dwlev = (long)pSkill->vectorLevels.size() >= lLevel? (DWORD)lLevel-1:((DWORD)pSkill->vectorLevels.size() - 1);
	long lAiID = pSkill->vectorLevels[dwlev].dwSkillLevelAI;
	vector<CSkillListXml::tagLevelValue> vectBegin = pSkill->vectorLevels[dwlev].vecStepBegin;
	m_pSkillAI = CSkillAI::CreateSkillAI(this, lAiID);
	vector<CSkillListXml::tagLevelValue>::iterator it;
	for(it = vectBegin.begin(); it!=vectBegin.end();it++)
	{
		ExecuteStep(it->dwID, it->dwValue, it->strValue);
	}
	// 技能开始时技能图标变灰冷却开启
	if(m_pMoveShape == GetGame()->GetMainPlayer())
	{
		// 冷却图标
		CPlayer::tagSkill *pS = GetGame()->GetMainPlayer()->GetSkill(lID);
		if(pS)
		{
			if(pS->dwRestoreTime>0)
			{
				pS->dwCoolDownStartTime = timeGetTime();
				pS->dwCoolDownGrayStartTime = 0;
			}
		}
		// 有吟唱特效和时间，更新吟唱条的显示数据
		if(m_lIntonateTime > 0)
		{
			DWORD dwCurTime = timeGetTime();
			GetGame()->GetMainPlayer()->SetIntonateSkill(lID);
			GetGame()->GetMainPlayer()->SetCurIntonateTime(m_lIntonateTime,dwCurTime);
			// 如果是引导类，反方向显示吟唱条
			if(pSkill->bIsLead)
				GetGame()->GetMainPlayer()->SetIsLToR(false);
			else
				GetGame()->GetMainPlayer()->SetIsLToR(true);
		}
	}
	if(m_pMoveShape->GetType() == TYPE_PET)
	{
		CPet *pPet = (CPet*)m_pMoveShape;
		if (pPet->IsMainPlayerPet())
		{
			//更新当前技能的开始冷却时间
			tagPetSkillInfo *pPetSkill = pPet->GetPetSkillInfoByID(lID);
			if(pPetSkill && pPetSkill->lRestorTimeLength>0)
			{
				pPetSkill->lStartCoolTime = GetCurTickCount();
			}
			//更新吟唱条
			// 
			if(m_lIntonateTime > 0)
			{
				DWORD dwCurTime = timeGetTime();
				pPet->SetIntonateSkill(lID);
				pPet->SetCurIntonateTime(m_lIntonateTime,dwCurTime);
				// 如果是引导类，反方向显示吟唱条
				if(pSkill->bIsLead)
					pPet->SetIsLToR(false);
				else
					pPet->SetIsLToR(true);
			}
		}
	}
	if(m_pSkillAI && m_pSkillAI->StepBeginAI(vType, vID))
	{
		return TRUE;
	}
	else
		return FALSE;
}

// 设置吟唱时间
void CSkillXml::IntonateTime(long lTime)
{
	m_lIntonateTime = lTime;
}

// 设置吟唱阶段主要循环动作
void CSkillXml::IntonateMainAct(WORD l)
{
	m_wIntonateMainAct = l;
}

// 设置吟唱前期动作ID
void CSkillXml::IntonateAct(long l)
{
	if(l)
	{
		DWORD dwCurrentTime = timeGetTime();
		AnimInfo::tagActionInfo *pActionInfo = m_pMoveShape->GetAnimInfo()->GetActionInfo();
		pActionInfo->bInterpolation = TRUE;
		pActionInfo->dwOldAction = pActionInfo->dwCurAction;
		pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
		pActionInfo->fOldActionSpeedRate = pActionInfo->fCurActionSpeedRate;
		pActionInfo->dwInterpolationElapseTime = 0;
		pActionInfo->dwInterpolationStartTime  = dwCurrentTime;
		pActionInfo->dwInterpolationTimeLength =240;
		m_pMoveShape->SetAction((WORD)l);
		//pActionInfo->bInterpolation = FALSE;
		
	}
}
// 设置吟唱动作特效ID
void CSkillXml::IntonateEffect(long l)
{
	m_lIntonateEffect = l;
}
// 设置吟唱特效索引
void CSkillXml::IntonateEffectIndex(long l)
{
	m_lIntonateEffectIndex = l;
}
// 吟唱特效是否与武器相关
void CSkillXml::IntonateEffectByEquip(long l)
{
	m_bIntonateEffectByEquip = ( l == 0)?false : true;
}
// 设置吟唱特效绑定位置
void CSkillXml::IntonateEffectPosition(long l)
{
	switch(l)
	{
	case 0: m_eIntonateEffectPosition = LOCATOR_WIND; break;
	case 1: m_eIntonateEffectPosition = LOCATOR_HEAD; break;
	case 2: m_eIntonateEffectPosition = LOCATOR_FOOTCENTER; break;
	case 3: m_eIntonateEffectPosition = LOCATOR_LEFTHAND; break;
	case 4: m_eIntonateEffectPosition = LOCATOR_RIGHTHAND; break;
	case 5: m_eIntonateEffectPosition = LOCATOR_LEFTFOOT; break;
	case 6: m_eIntonateEffectPosition = LOCATOR_RIGHTFOOT; break;
	default: m_eIntonateEffectPosition = LOCATOR_FOOTCENTER; break;
	}
}
// 设置吟唱特效是否循环
void CSkillXml::IntonateEffectCycle(long l)
{
	m_bIntonateEffectCycle = (l == 0)? false: true;
}
// 开始吟唱
void CSkillXml::IntonatePlay()
{
	if(m_lIntonateEffect)
	{
		// 吟唱特效是否与装备相关
		if (m_bIntonateEffectByEquip)
		{
			if(m_pMoveShape->GetType() == TYPE_PLAYER)		// 玩家
			{
				DWORD dwIndex = 0;
				CPlayer *pPlayer = dynamic_cast<CPlayer*>(m_pMoveShape);
				if (GetGame()->GetMainPlayer() == m_pMoveShape)				// 当前玩家
				{
					CGoods *pGoods = pPlayer->GetEquip(CPlayer::EQUIP_WEAPON);
					if(pGoods)
						dwIndex = pGoods->GetProperty()->dwGroundId;
				}
				else	// 其他玩家
				{
					DWORD dwWeapon = pPlayer->GetEquipIndex(CPlayer::EQUIP_WEAPON);
					CGoodsList::tagGoods2 *pGP = CGoodsList::GetProperty(dwWeapon);
					if(pGP)
					{
						dwIndex = pGP->BaseProperty.dwGroundId;
					}
				}
				m_lIntonateEffect = CWeaponEffectXml::GetWeaponEffect(dwIndex);
			}
		}
		// 其他怪物以配置设定播放
		bool bAsyn = true;
		// 自己是技能释放者
		if(GetGame()->GetMainPlayer() == m_pMoveShape)
			bAsyn = false;
		long lType = m_pMoveShape->GetType();
		CGUID guidEffect = CGUID::GUID_INVALID;
		if(m_pMoveShape)
			guidEffect = m_pMoveShape->AddEffectLocator(m_lIntonateEffect,m_pMoveShape,m_eIntonateEffectPosition,m_bIntonateEffectCycle,0.f,0,bAsyn);
		// 循环特效，且存在删除延迟时间
		if(m_bIntonateEffectCycle && m_lCycleEffectDelayTime > 0)
		{
			CEffect * pEffect = m_pMoveShape->FindEffect(m_lIntonateEffect);
			if(pEffect)
			{
				pEffect->SetIsNeedAlphaFade(true);
				pEffect->SetAlphaFadeInTimeLength(m_lCycleEffectDelayTime);
			}
		}
		
		//=====================07.12.19刘科===================
		if(lType != 0 && guidEffect != CGUID::GUID_INVALID)
		{
			// 构造一个新的特效索引结构
			tagEffectIndex stEffectIndex;
			stEffectIndex.guidEffect = guidEffect;
			stEffectIndex.lEffectIndex = m_lIntonateEffectIndex;
			// 在本技能步骤下，查找关于本目标的特效列表，如果找到就直接添加进去，否则，构造一个新的对象特效结构添加到本阶段列表中去
			list<CSkillXml::tagEffectIndex> *lsEffectIndex = GetObjectEffectList(m_lStep,lType,m_pMoveShape->GetExID());
			if(lsEffectIndex)
			{
				lsEffectIndex->push_back(stEffectIndex);
			}
			else
			{
				vector<CSkillXml::tagObjectEffect> *vecObjEffect = GetEffectIndex(m_lStep);
				if(vecObjEffect)
				{
					tagObjectEffect stObjEffect;
					stObjEffect.guid = m_pMoveShape->GetExID();
					stObjEffect.lType = lType;
					stObjEffect.lsEffectIndex.push_back(stEffectIndex);
					vecObjEffect->push_back(stObjEffect);
				}
			}
			//tagEffectIndex stEffectIndex;
			//stEffectIndex.guidEffect = guidEffect;
			//stEffectIndex.lEffectIndex = m_lIntonateEffectIndex;
			//// 首先查看本阶段的特效列表中是否有关于该对象的特效列表，如果有，就在对象的特效列表中直接添加，否则需要构造一个新的对象特效
			//for(int i=0; i<m_vecEffectIndex[m_lStep].size(); ++i)
			//{
			//	if((m_vecEffectIndex[m_lStep][i]).guid == m_pMoveShape->GetExID() && (m_vecEffectIndex[m_lStep][i]).lType)
			//	{
			//		(m_vecEffectIndex[m_lStep][i]).lsEffectIndex.push_back(stEffectIndex);
			//		return;
			//	}
			//}
			//tagObjectEffect stObjEffect;
			//stObjEffect.guid = m_pMoveShape->GetExID();
			//stObjEffect.lType = lType;
			//stObjEffect.lsEffectIndex.push_back(stEffectIndex);
			//m_vecEffectIndex[m_lStep].push_back(stObjEffect);
		}
		//====================================================
	}
		
}
// 设置吟唱声效id
void CSkillXml::IntonateSound(long l)
{
	m_lIntonateSound = l;
}
// 吟唱音效是否与武器相关
void CSkillXml::IntonateSoundByEquip(long l)
{
	m_bIntonateSoundByEquip = (l == 0)?false:true;
}
// 设置吟唱声效是否循环
void CSkillXml::IntonateSoundCycle(long l)
{
	m_bIntonateSoundCycle = (l == 0)? false:true;
}
// 开始播放吟唱声效
void CSkillXml::IntonateSoundPlay()
{
	if(m_lIntonateSound)
	{
		//判断男女声效,若小于5位数，则添加1位，1为男声，2为女声
		if(((int)m_lIntonateSound-10000)<0&&m_pMoveShape->GetType()==TYPE_PLAYER)
		{
			CPlayer *pPlayerOther = dynamic_cast<CPlayer*>(m_pMoveShape);
			if(pPlayerOther->GetSex())
			{
				m_lIntonateSound = m_lIntonateSound*10+2;
			}else{
				m_lIntonateSound = m_lIntonateSound*10+1;
			}
		}


		// 吟唱音效是否与装备有关
		if(m_bIntonateSoundByEquip)
		{
			if (m_pMoveShape->GetType() == TYPE_PLAYER)				// 玩家
			{
				DWORD dwIndex = 0;
				CPlayer *pPlayer = dynamic_cast<CPlayer*>(m_pMoveShape);
				if (GetGame()->GetMainPlayer() == m_pMoveShape)
				{
					CGoods *pWeapon = pPlayer->GetEquip(CPlayer::EQUIP_WEAPON);
					if(pWeapon)
						dwIndex = pWeapon->GetIndex();
				}
				else
				{
					dwIndex = pPlayer->GetEquipIndex(CPlayer::EQUIP_WEAPON);
				}
				DWORD dwSoundID1 = 0, dwSoundID2 = 0;
				CGoodsList::tagGoods2 *pGoods = CGoodsList::GetProperty(dwIndex);
				if (pGoods)
				{
					// 播放装备物品设定的两种音效
					dwSoundID1 = pGoods->BaseProperty.dwSoundID1;
					dwSoundID2 = pGoods->BaseProperty.dwSoundID2;
					GetGame()->GetAudioList()->Play(NULL, TYPE_SKILL, (WORD)dwSoundID1, Vector3(pPlayer->GetPosY(), pPlayer->GetHeight(),
						pPlayer->GetPosX()),m_bIntonateSoundCycle);
					if (dwSoundID2)
					{
						GetGame()->GetAudioList()->Play(NULL, TYPE_SKILL, (WORD)dwSoundID2, Vector3(pPlayer->GetPosY(), 
							pPlayer->GetHeight(), pPlayer->GetPosX()),m_bIntonateSoundCycle);
					}
				}
			}
			else		// 其他的就播放配置中设定的音效 

				m_IntonateSoundChannel = GetGame()->GetAudioList()->Play(m_pMoveShape,TYPE_SKILL,(WORD)m_lIntonateSound, Vector3(m_pMoveShape->GetPosY(),
				m_pMoveShape->GetHeight(),m_pMoveShape->GetPosX()),m_bIntonateSoundCycle);
		}
		else	// 如果不与装备相关就播放配置中设定的音效
			m_IntonateSoundChannel = GetGame()->GetAudioList()->Play(m_pMoveShape,TYPE_SKILL,(WORD)m_lIntonateSound, Vector3(m_pMoveShape->GetPosY(),
			m_pMoveShape->GetHeight(),m_pMoveShape->GetPosX()),m_bIntonateSoundCycle);	
	}
}
// 设置是否可以被打断
void CSkillXml::IntonateBreak(long l)
{
	GetGame()->GetGameControl()->SetIntonateBreak((l == 0)?false:true);
}


//////////////////////////////////////////////////////////////////////////
// 执行技能实施步骤
//////////////////////////////////////////////////////////////////////////
bool CSkillXml::StepRun(long lID, long lLevel, long sType, const CGUID& sID,long dX, long dY, float fDir, vector<long> vType, vector<CGUID> vID)
{
	m_lStep = 1;
	m_pMoveShape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(sType, sID));
	m_fDir = fDir;
	vector<long>::iterator itShape;
	int index = 0;
	if(m_vecDestMoveShape.size()>0)
		m_vecDestMoveShape.clear();
	for(itShape = vType.begin(); itShape!=vType.end(); itShape++)
	{
		CMoveShape* moveshape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject((*itShape),vID[index]));
		if(moveshape)
			m_vecDestMoveShape.push_back(moveshape);
		index++;
	}
	m_lSourType = sType;
	m_SourID = sID;
	m_lDestType = (vType.size()>0)?vType.back():0;
	m_DestID = (vID.size()>0)?vID.back():CGUID::GUID_INVALID;
	m_lDestX = dX;
	m_lDestY = dY;
	m_lSkillID = lID;
	m_lSkillLevel = lLevel;
	// 执行开始步骤
	CSkillListXml::tagSkill *pSkill = CSkillListXml::GetProperty(lID);
	if(pSkill == NULL)
		return FALSE;
	DWORD dwlev = (long)pSkill->vectorLevels.size() >= lLevel? (DWORD)lLevel-1:((DWORD)pSkill->vectorLevels.size() - 1);
	vector<CSkillListXml::tagLevelValue> vectRun = pSkill->vectorLevels[dwlev].vecStepRun;
	SAFE_DELETE(m_pSkillAI);
	m_pSkillAI = CSkillAI::CreateSkillAI(this, pSkill->vectorLevels[dwlev].dwSkillLevelAI);
	vector<CSkillListXml::tagLevelValue>::iterator it;
	for (it = vectRun.begin(); it != vectRun.end(); it++) 
	{
		ExecuteStep(it->dwID, it->dwValue, it->strValue);
	}
	//////////////////////////////////////////////////////////////////////////
	// 测试召唤技能代码
	vector<CSkillListXml::tagLevelValue> vecSummon = pSkill->vectorLevels[dwlev].vecStepSummon;
	if(vecSummon.size()>0)
	{
		//m_pSkillAI = CSkillAI::CreateSkillAI(this, pSkill->vectorLevels[dwlev].dwSkillLevelAI);
		for (it = vecSummon.begin(); it != vecSummon.end(); it++)
		{
			ExecuteStep(it->dwID, it->dwValue, it->strValue);
		}
	}
	// 本人引导类，反方向显示吟唱条
	if(m_pMoveShape == GetGame()->GetMainPlayer() && m_lIntonateTime > 0 && pSkill->bIsLead)
	{
		// 有吟唱特效和时间，更新吟唱条的显示数据
		DWORD dwCurTime = timeGetTime();
		GetGame()->GetMainPlayer()->SetIntonateSkill(lID);
		GetGame()->GetMainPlayer()->SetCurIntonateTime(m_lIntonateTime,dwCurTime);
		GetGame()->GetMainPlayer()->SetIsLToR(false);
	}
	// 删除前一个阶段的所有特效
	DelAllEffectListByStep(STEP_START);
	//////////////////////////////////////////////////////////////////////////
	if (m_pSkillAI && m_pSkillAI->StepRunAI(vType,vID) == false)
		return FALSE;
	else
		return TRUE;
}
// 设置施放动作ID
void CSkillXml::ReleaseAct(long l)
{
	m_pMoveShape->SetDirEx(m_fDir);
	if(l)
	{
		extern bool g_bForceUpdate;
		g_bForceUpdate = true;
		// 如果技能为普通攻击
		if(l == ACT_ATTACK)
		{
			m_pMoveShape->SetAttackAction(random(3));
			m_pMoveShape->SetAction((WORD)l);
		}
		else   
		{
   			m_pMoveShape->SetAction((WORD)l);
		}
	}
}

// 设置施放后续动作ID
void CSkillXml::ReleaseActEx(long l)
{
	if (l)
		m_queueRunStepActEx.push((WORD)l);
}

// 设置施放动作特效id
void CSkillXml::ReleaseEffect(long l)
{
	m_lReleaseEffect = l;
}
// 设置动作特效索引
void CSkillXml::ReleaseEffectIndex(long l)
{
	m_lReleaseEffectIndex = l;
}
// 施放特效是否与武器相关
void CSkillXml::ReleaseEffectByEquip(long l)
{
	m_bReleaseEffectByEquip = (l == 0)?false:true;
}
// 设置施放特效目标
void CSkillXml::ReleaseEffectDest(long l)
{
	m_lReleaseEffectDest = l;
}
// 设置施放特效绑定位置
void CSkillXml::ReleaseEffectPosition(long l)
{
	switch(l)
	{
	case 0: m_eReleaseEffectPosition = LOCATOR_WIND; break;
	case 1: m_eReleaseEffectPosition = LOCATOR_HEAD; break;
	case 2: m_eReleaseEffectPosition = LOCATOR_FOOTCENTER; break;
	case 3: m_eReleaseEffectPosition = LOCATOR_LEFTHAND; break;
	case 4: m_eReleaseEffectPosition = LOCATOR_RIGHTHAND; break;
	case 5: m_eReleaseEffectPosition = LOCATOR_LEFTFOOT; break;
	case 6: m_eReleaseEffectPosition = LOCATOR_RIGHTFOOT; break;
	default: m_eReleaseEffectPosition = LOCATOR_FOOTCENTER; break;
	}
}
// 设置施放特效是否循环
void CSkillXml::ReleaseEffectCycle(long l)
{
	m_bReleaseEffectCycle = (l == 0)?false : true;
}
// 开始施放动作
void CSkillXml::ReleasePlay()
{
	bool bAsyn = true;
	// 自己是技能释放者
	CMoveShape* pMoveshape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(GetSourType(), GetSourID()));
	if(GetGame()->GetMainPlayer() == pMoveshape)
		bAsyn = false;
	if(m_lReleaseEffect)
	{
		// 施放特效是否与装备相关
		if (m_bReleaseEffectByEquip)
		{
			if (m_pMoveShape->GetType() == TYPE_PLAYER)				// 玩家
			{
				DWORD dwIndex = 0;
				CPlayer *pPlayer = dynamic_cast<CPlayer*>(m_pMoveShape);
				if (GetGame()->GetMainPlayer() == m_pMoveShape)		// 当前玩家
				{
					CGoods *pGoods = pPlayer->GetEquip(CPlayer::EQUIP_WEAPON);
					if(pGoods)
						dwIndex = pGoods->GetProperty()->dwGroundId;
				}
				else	// 其他玩家
				{
					DWORD dwWeapon = pPlayer->GetEquipIndex(CPlayer::EQUIP_WEAPON);
					CGoodsList::tagGoods2 *pGP = CGoodsList::GetProperty(dwWeapon);
					if (pGP)
					{
						dwIndex = pGP->BaseProperty.dwGroundId;
					}
				}
				m_lIntonateEffect = CWeaponEffectXml::GetWeaponEffect(dwIndex);
			}
		}
		// 临时变量，用于保存特效存放者的类型和GUID，特效自身的GUID
		long lType = 0;
		CGUID guidObj = CGUID::GUID_INVALID;
		CGUID guidEffect = CGUID::GUID_INVALID;
		CEffect * pEffect = NULL;
		switch(m_lReleaseEffectDest)
		{
			// 播放特效 (对自己)
		case 0:
			if(m_pMoveShape)
			{
				lType = m_pMoveShape->GetType();
				guidObj = m_pMoveShape->GetExID();
				guidEffect = m_pMoveShape->AddEffectLocator(m_lReleaseEffect, m_pMoveShape, m_eReleaseEffectPosition, m_bReleaseEffectCycle, m_pMoveShape->GetNowDir(), m_lReleaseEffectIndex,bAsyn);
				pEffect = m_pMoveShape->FindEffect(m_lReleaseEffect,m_lReleaseEffectIndex);
			}
			break;
			// 播放特效 (对目标)
		case 1:
			if(m_vecDestMoveShape.size()>0)
			{
				vector<CMoveShape*>::iterator it;
				for (it = m_vecDestMoveShape.begin(); it!=m_vecDestMoveShape.end(); it++)
				{
					if((*it))
					{
						lType = (*it)->GetType();
						guidObj = (*it)->GetExID();
						guidEffect = (*it)->AddEffectLocator(m_lReleaseEffect, (*it), m_eReleaseEffectPosition, m_bReleaseEffectCycle, m_pMoveShape->GetNowDir(), m_lReleaseEffectIndex,bAsyn);
						pEffect = (*it)->FindEffect(m_lReleaseEffect,m_lReleaseEffectIndex);
					}
				}
			}
			else
			{
				lType = TYPE_REGION;
				guidObj = GetGame()->GetRegion()->GetExID();
				pEffect = GetGame()->GetRegion()->PlayEffect(m_lReleaseEffect, 
					m_lDestX+0.5f, m_lDestY+0.5f, 
					GetGame()->GetRegion()->GetCellHeight((float)m_lDestX,(float) m_lDestY), 
					m_pMoveShape->GetNowDir(),bAsyn);
				pEffect->SetLoop(m_bReleaseEffectCycle);
				if(pEffect)
					guidEffect = pEffect->GetExID();
			}
			break;
			// 播放特效 (对地面)
		case 2: 
			lType = TYPE_REGION;
			guidObj = GetGame()->GetRegion()->GetExID();
			pEffect = GetGame()->GetRegion()->PlayEffect(m_lReleaseEffect, 
				m_lDestX+0.5f, m_lDestY+0.5f,
				GetGame()->GetRegion()->GetCellHeight((float)m_lDestX,(float) m_lDestY),
				-1.f,bAsyn);
			pEffect->SetLoop(m_bReleaseEffectCycle);
			if(pEffect)
				guidEffect = pEffect->GetExID();
			break;
			// 播放特效-本格
		case 3: 
			if(m_pMoveShape)
			{
				lType = TYPE_REGION;
				guidObj = GetGame()->GetRegion()->GetExID();
				pEffect = GetGame()->GetRegion()->PlayEffect(m_lReleaseEffect, m_pMoveShape->GetPosX(), m_pMoveShape->GetPosY(),  GetGame()->GetRegion()->GetCellHeight(m_pMoveShape->GetPosX(), m_pMoveShape->GetPosY()), m_pMoveShape->GetNowDir(),bAsyn);
				if(pEffect)
					guidEffect = pEffect->GetExID();
			}
			break;
		default: 
			break;
		}
		// 循环特效，且存在删除延迟时间
		if(pEffect && m_bReleaseEffectCycle && m_lCycleEffectDelayTime > 0)
		{
			pEffect->SetIsNeedAlphaFade(true);
			pEffect->SetAlphaFadeInTimeLength(m_lCycleEffectDelayTime);
		}
		//=====================07.12.19刘科===================
		if(lType != 0 && guidEffect != CGUID::GUID_INVALID && guidObj != CGUID::GUID_INVALID)
		{
			// 构造一个新的特效索引结构
			tagEffectIndex stEffectIndex;
			stEffectIndex.guidEffect = guidEffect;
			stEffectIndex.lEffectIndex = m_lReleaseEffectIndex;
			// 在本技能步骤下，查找关于本场景的特效列表，如果找到就直接添加进去，否则，构造一个新的对象特效结构添加到本阶段列表中去
			list<CSkillXml::tagEffectIndex> *lsEffectIndex = GetObjectEffectList(m_lStep,lType,guidObj);
			if(lsEffectIndex)
			{
				lsEffectIndex->push_back(stEffectIndex);
			}
			else
			{
				vector<CSkillXml::tagObjectEffect> *vecObjEffect = GetEffectIndex(m_lStep);
				if(vecObjEffect)
				{
					tagObjectEffect stObjEffect;
					stObjEffect.guid = guidObj;
					stObjEffect.lType = lType;
					stObjEffect.lsEffectIndex.push_back(stEffectIndex);
					vecObjEffect->push_back(stObjEffect);
				}
			}
			//tagEffectIndex stEffectIndex;
			//stEffectIndex.guidEffect = guidEffect;
			//stEffectIndex.lEffectIndex = m_lReleaseEffectIndex;
			//// 首先查看本阶段的特效列表中是否有关于该对象的特效列表，如果有，就在对象的特效列表中直接添加，否则需要构造一个新的对象特效
			//for(int i=0; i<m_vecEffectIndex[m_lStep].size(); ++i)
			//{
			//	if((m_vecEffectIndex[m_lStep][i]).guid == guidObj && (m_vecEffectIndex[m_lStep][i]).lType)
			//	{
			//		(m_vecEffectIndex[m_lStep][i]).lsEffectIndex.push_back(stEffectIndex);
			//		return;
			//	}
			//}
			//tagObjectEffect stObjEffect;
			//stObjEffect.guid = guidObj;
			//stObjEffect.lType = lType;
			//stObjEffect.lsEffectIndex.push_back(stEffectIndex);
			//m_vecEffectIndex[m_lStep].push_back(stObjEffect);
		}
		
		//====================================================
	}
	
}
// 设置施放声效id
void CSkillXml::ReleaseSound(long l)
{
	m_lReleaseSound = l;
}
// 施放音效是否与武器相关
void CSkillXml::ReleaseSoundByEquip(long l)
{
	m_bReleaseSoundByEquip = (l == 0)?false:true;
}
// 设置施放声效是否循环
void CSkillXml::ReleaseSoundCycle(long l)
{
	m_bReleaseSoundCycle = (l == 0)?false:true;
}
// 设置施放声效目标
void CSkillXml::ReleaseSoundDest(long l)
{
	m_lReleaseSoundDest = l;
}
// 播放施放声效
void CSkillXml::ReleaseSoundPlay()
{
	if(m_lReleaseSound)
	{
		//关闭呤唱声音效果
		char strbuf[MAX_PATH]="";
		sprintf(strbuf,	"sounds/skills/%03d.wav", m_lIntonateSound);
		if(GetGame()->GetAudioList()->IsPlaying(strbuf,m_IntonateSoundChannel))
		{
			GetGame()->GetAudioList()->Stop3DSound(strbuf,m_IntonateSoundChannel);
		}
		//判断男女声效，若小于5位数，则添加1位，1为男声，2为女声
		if(((int)m_lReleaseSound-10000)<0&&m_pMoveShape->GetType()==TYPE_PLAYER)
		{
			CPlayer *pPlayerOther = dynamic_cast<CPlayer*>(m_pMoveShape); 
			if(pPlayerOther->GetSex())
			{
				m_lReleaseSound = m_lReleaseSound*10+2;
			}else{
				m_lReleaseSound = m_lReleaseSound*10+1;
			}
		}


		// 施放声效是否与装备有关
		if (m_bReleaseSoundByEquip)
		{
			if(m_pMoveShape->GetType() == TYPE_PLAYER)			// 玩家
			{
				DWORD dwIndex = 0;
				CPlayer *pPlayer = dynamic_cast<CPlayer*>(m_pMoveShape);
				if(GetGame()->GetMainPlayer() == m_pMoveShape)		// 当前玩家
				{
					CGoods *pWeapon = pPlayer->GetEquip(CPlayer::EQUIP_WEAPON);
					if(pWeapon)
						dwIndex = pWeapon->GetIndex();
				}
				else	// 其他玩家
				{
					dwIndex = pPlayer->GetEquipIndex(CPlayer::EQUIP_WEAPON);
				}
				DWORD dwSoundID1 = 0, dwSoundID2 = 0;
				CGoodsList::tagGoods2 *pGoods = CGoodsList::GetProperty(dwIndex);		// 取得装备有关的声效
				if (pGoods)
				{
					// 播放装备物品设定的两种音效
					dwSoundID1 = pGoods->BaseProperty.dwSoundID1;
					dwSoundID2 = pGoods->BaseProperty.dwSoundID2;
					GetGame()->GetAudioList()->Play(NULL, TYPE_SKILL, (WORD)dwSoundID1,Vector3(pPlayer->GetPosY(),pPlayer->GetHeight(),
						pPlayer->GetPosX()),m_bReleaseSoundCycle);
					if (dwSoundID2)
					{
						GetGame()->GetAudioList()->Play(NULL, TYPE_SKILL, (WORD)dwSoundID2,Vector3(pPlayer->GetPosY(),pPlayer->GetHeight(),
							pPlayer->GetPosX()),m_bReleaseSoundCycle);
					}
				}
			}
			else	// 其他怪物
			{
				switch(m_lReleaseSoundDest)
				{
					char strFile[MAX_PATH];
				case 0:// 对自己播放音效
					GetGame()->GetAudioList()->Play(m_pMoveShape,TYPE_SKILL,(WORD)m_lReleaseSound, Vector3(m_pMoveShape->GetPosY(),
						m_pMoveShape->GetHeight(),m_pMoveShape->GetPosX()),m_bReleaseSoundCycle);
					break;
				case 1:// 对目标播放音效
					{
						sprintf(strFile, "sounds/skills/%d.wav",m_lReleaseSound);
						if (m_vecDestMoveShape.size()>0)
						{
							vector<CMoveShape*>::iterator it;
							for (it = m_vecDestMoveShape.begin(); it!=m_vecDestMoveShape.end(); it++)
							{
								GetGame()->GetAudioList()->Play(strFile, (*it)->GetPosX(),(*it)->GetPosY(),
									(*it)->GetHeight(),m_bReleaseSoundCycle);
							}
						}
						else
						{
							CClientRegion* pRegion = (CClientRegion*)m_pMoveShape->GetFather();
							float x = (float)m_lDestX + 0.5f;
							float y = (float)m_lDestY + 0.5f;
							GetGame()->GetAudioList()->Play(strFile, x, y, pRegion->GetCellHeight(x, y),m_bReleaseSoundCycle);
						}
					}
					break;
				case 2:// 对地面播放音效
					{
						sprintf(strFile, "sounds/skills/%d.wav", m_lReleaseSound);
						CClientRegion* pRegion = (CClientRegion*)m_pMoveShape->GetFather();
						float x = (float)m_lDestX + 0.5f;
						float y = (float)m_lDestY + 0.5f;
						GetGame()->GetAudioList()->Play(strFile, x, y, pRegion->GetCellHeight(x, y),m_bReleaseSoundCycle);
					}
					break;
				default: break;
				}
			}
		}
		else	// 不与装备有关
		{
			switch(m_lReleaseSoundDest)
			{
				char strFile[MAX_PATH];
			case 0:// 对自己播放音效
				GetGame()->GetAudioList()->Play(m_pMoveShape,TYPE_SKILL,(WORD)m_lReleaseSound, Vector3(m_pMoveShape->GetPosY(),
					   m_pMoveShape->GetHeight(),m_pMoveShape->GetPosX()),m_bReleaseSoundCycle);
				break;
			case 1:// 对目标播放音效
				{
					sprintf(strFile, "sounds/skills/%d.wav",m_lReleaseSound);
					if (m_vecDestMoveShape.size()>0)
					{
						vector<CMoveShape*>::iterator it;
						for (it = m_vecDestMoveShape.begin(); it!=m_vecDestMoveShape.end(); it++)
						{
							GetGame()->GetAudioList()->Play(strFile, (*it)->GetPosX(),(*it)->GetPosY(),
								(*it)->GetHeight(),m_bReleaseSoundCycle);
						}
					}
					else
					{
						CClientRegion* pRegion = (CClientRegion*)m_pMoveShape->GetFather();
						float x = (float)m_lDestX + 0.5f;
						float y = (float)m_lDestY + 0.5f;
						GetGame()->GetAudioList()->Play(strFile, x, y, pRegion->GetCellHeight(x, y),m_bReleaseSoundCycle);
					}
				}
				break;
			case 2:// 对地面播放音效
				{
					sprintf(strFile, "sounds/skills/%d.wav", m_lReleaseSound);

					CClientRegion* pRegion = (CClientRegion*)m_pMoveShape->GetFather();
					float x = (float)m_lDestX + 0.5f;
					float y = (float)m_lDestY + 0.5f;
					GetGame()->GetAudioList()->Play(strFile, x, y, pRegion->GetCellHeight(x, y),m_bReleaseSoundCycle);
				}
				break;
			default: break;
			}
		}
	}
}
// 震动频率(百分比)
void CSkillXml::ShockRatio(long l)
{
	m_fShockRatio = (float)(l/100.0f);
}
// 震动间隔时
void CSkillXml::ShockInterval(long l)
{
	m_lShockInterval = l;
}
// 震动时间长度
void CSkillXml::ShockTimeLength(long l)
{
	m_lShockTimeLength = l;
}



/*
 * 功能: 开始震动
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.01.17 - lpf
 *		增加了对系统设置里是否开启震动开关的判断
 */
void CSkillXml::ShockPlay()
{
	if (GetGame()->GetSetup()->bShock &&
		m_fShockRatio && m_lShockInterval && m_lShockTimeLength)
	{
		GetGame()->GetRegion()->GetRegionCamera()->ShockCamera(m_fShockRatio, m_lShockInterval, m_lShockTimeLength);
	}
}



// 设置飞行特效ID
void CSkillXml::FlyEffect(long l)
{
	m_lFlyEffect = l;
}
// 设置飞行特效索引
void CSkillXml::FlyEffectIndex(long l)
{
	m_lFlyEffectIndex = l;
	m_vecFlyEffectIndex.push_back(m_lFlyEffectIndex);
}
// 飞行特效是否与武器相关
void CSkillXml::FlyEffectByEquip(long l)
{
	m_bFlyEffectByEquip = (l == 0)?false:true;
}
// 设置飞行特效从何处发出
void CSkillXml::FlyEffectPosition(long l)
{
	switch(l)
	{
	case 0: m_eFlyEffectPosition = LOCATOR_WIND; break;
	case 1: m_eFlyEffectPosition = LOCATOR_HEAD; break;
	case 2: m_eFlyEffectPosition = LOCATOR_FOOTCENTER; break;
	case 3: m_eFlyEffectPosition = LOCATOR_LEFTHAND; break;
	case 4: m_eFlyEffectPosition = LOCATOR_RIGHTHAND; break;
	case 5: m_eFlyEffectPosition = LOCATOR_LEFTFOOT; break;
	case 6: m_eFlyEffectPosition = LOCATOR_RIGHTFOOT; break;
	default: m_eFlyEffectPosition = LOCATOR_FOOTCENTER; break;
	}
	m_vecFlyEffectPos.push_back(m_eFlyEffectPosition);
}
// 设置飞行特效是否循环播放
void CSkillXml::FlyEffectCycle(long l)
{
	m_bFlyEffectCycle = (l == 0)?false : true;
}
// 设置飞行特效X轴方向偏移角度
void CSkillXml::FlyEffectTurnX(long l)
{
	m_lFlyEffectTurnX = l;
	m_vecFlyEffectTurnX.push_back(l);
}
// 设置飞行特效Y轴方向偏移角度
void CSkillXml::FlyEffectTurnY(long l)
{
	m_lFlyEffectTurnY = l;
	m_vecFlyEffectTurnY.push_back(l);
}
// 开始飞行动作
void CSkillXml::FlyPlay()
{
	if(m_pMoveShape && m_lFlyEffect )
	{
		if (m_bFlyEffectByEquip)			// 飞行特效是否与装备相关
		{
			if(m_pMoveShape->GetType() == TYPE_PLAYER)	// 玩家
			{
				DWORD dwIndex = 0;
				CPlayer *pPlayer = dynamic_cast<CPlayer*>(m_pMoveShape);
				if (GetGame()->GetMainPlayer() == m_pMoveShape)		// 当前玩家
				{
					CGoods *pGoods = pPlayer->GetEquip(CPlayer::EQUIP_WEAPON);
					if (pGoods)
						dwIndex = pGoods->GetProperty()->dwGroundId;
				}
				else	// 其他玩家
				{
					DWORD dwWeapon = pPlayer->GetEquipIndex(CPlayer::EQUIP_WEAPON);
					CGoodsList::tagGoods2 *pGP = CGoodsList::GetProperty(dwWeapon);
					if (pGP)
					{
						dwIndex = pGP->BaseProperty.dwGroundId;
					}
				}
				m_lFlyEffect = CWeaponEffectXml::GetWeaponEffect(dwIndex);	// 从配置中取到特效编号
			}
		}
		m_vecFlyEffect.push_back(m_lFlyEffect);
	}
	
}

// 设置飞行音效id
void CSkillXml::FlySound(long l)
{
	m_lFlySound = l;
}
// 飞行音效是否与武器相关
void CSkillXml::FlySoundByEquip(long l)
{
	m_bFlySoundByEquip =(l == 0)?false:true;
}
// 设置飞行音效从何处释放
void CSkillXml::FlySoundPosition(long l)
{
	switch(l)
	{
	case 0: m_eFlySoundPosition = LOCATOR_WIND; break;
	case 1: m_eFlySoundPosition = LOCATOR_HEAD; break;
	case 2: m_eFlySoundPosition = LOCATOR_FOOTCENTER; break;
	case 3: m_eFlySoundPosition = LOCATOR_LEFTHAND; break;
	case 4: m_eFlySoundPosition = LOCATOR_RIGHTHAND; break;
	case 5: m_eFlySoundPosition = LOCATOR_LEFTFOOT; break;
	case 6: m_eFlySoundPosition = LOCATOR_RIGHTFOOT; break;
	default: m_eFlySoundPosition = LOCATOR_FOOTCENTER; break;
	}
}
// 设置飞行音效是否循环
void CSkillXml::FlySoundCycle(long l)
{
	m_bFlySoundCycle = (l == 0)?false:true;
}
// 开始播放飞行音效
void CSkillXml::FlySoundPlay()
{
	if(m_pMoveShape && m_lFlySound )
	{
		int nChannel = 0;
		if (m_bFlySoundByEquip)		// 飞行声效是否与装备相关
		{
			if (m_pMoveShape->GetType() == TYPE_PLAYER)		// 玩家
			{
				DWORD dwIndex = 0;
				CPlayer *pPlayer = dynamic_cast<CPlayer*>(m_pMoveShape);
				if (GetGame()->GetMainPlayer() == m_pMoveShape)		// 当前玩家
				{
					CGoods *pWeapon = pPlayer->GetEquip(CPlayer::EQUIP_WEAPON);
					if (pWeapon)
						dwIndex = pPlayer->GetEquipIndex(CPlayer::EQUIP_WEAPON);
				}
				else		// 其他玩家
				{
					dwIndex = pPlayer->GetEquipIndex(CPlayer::EQUIP_WEAPON);
				}
				DWORD dwSoundID1 = 0, dwSoundID2 = 0;
				CGoodsList::tagGoods2 *pGoods = CGoodsList::GetProperty(dwIndex);
				if (pGoods)
				{
					// 播放装备物品设定的两种音效
					dwSoundID1 = pGoods->BaseProperty.dwSoundID1;
					dwSoundID2 = pGoods->BaseProperty.dwSoundID2;
					GetGame()->GetAudioList()->Play(NULL, TYPE_SKILL, (WORD)dwSoundID1, Vector3(pPlayer->GetPosY(), pPlayer->GetHeight(),
						pPlayer->GetPosX()),m_bFlySoundCycle);
					if (dwSoundID2)
					{
						GetGame()->GetAudioList()->Play(NULL, TYPE_SKILL, (WORD)dwSoundID2, Vector3(pPlayer->GetPosY(), 
							pPlayer->GetHeight(), pPlayer->GetPosX()),m_bFlySoundCycle);
					}
				}
			}
			else		// 其他怪物播放配置中的声效
			{
				char strFile[MAX_PATH];
				sprintf(strFile, "sounds/skills/%d.wav", m_lFlySound); 
				float x, y, fHeight;
				m_pMoveShape->GetLocatorPos(m_eFlySoundPosition, x, y, fHeight);
				nChannel = GetGame()->GetAudioList()->Play(strFile, x, y, fHeight, m_bFlySoundCycle);
			}
		}
		else		// 播放配置中的声效
		{
			char strFile[MAX_PATH];
			sprintf(strFile, "sounds/skills/%d.wav", m_lFlySound);
			float x, y, fHeight;
			m_pMoveShape->GetLocatorPos(m_eFlySoundPosition, x, y, fHeight);
			nChannel = GetGame()->GetAudioList()->Play(strFile, x, y, fHeight, m_bFlySoundCycle);
		}
		SetFlySoundChannel(nChannel);
	}
}
// 设置飞行速度
void CSkillXml::FlyTime(long l)
{
	m_vecFlyTime.push_back(l);
}
// 设置飞行延时
void CSkillXml::FlyDelay(long l)
{
	 m_vecFlyDelay.push_back(l);
}

// 飞行特效的个数
void CSkillXml::FlyEffectNum(long l)
{
	m_lFlyEffectNum = l;
}

// 设置召唤特效队列
void CSkillXml::SummonEffect(long l)
{
	m_vecSummonEffect.push_back(l);
}
// 设置召唤声效
void CSkillXml::SummonSound(long l)
{
	m_vecSummonSound.push_back(l);
}
// 设置召唤生命周期
void CSkillXml::SummonLifeTime(long l)
{
	m_lSummonLifeTime = l ;
}
// 设置随机召唤技能X轴范围
void CSkillXml::RandomSummonX(long l)
{
	m_lRandomSummonX = l;
}
// 设置随机召唤技能Y轴范围
void CSkillXml::RandomSummonY(long l)
{
	m_lRandomSummonY = l;
}
// 设置随机召唤技能召唤物数量
void CSkillXml::RandomSummonNum(long l)
{
	m_lRandomSummonNum = l;
}

// 设置随机召唤技能召唤物的延迟播放时间
void CSkillXml::RandomSummonDelayTime(long l)
{
	m_lRandomSummonDelayTime = l;
}

// 设置是否显示挥动武器的光影效果
void CSkillXml::WeaponEffect(long l)
{
	// 非0就设置武器光影效果
	if(l > 0 && m_pMoveShape && m_pMoveShape->GetType() == TYPE_PLAYER)
	{
		// 获得当前玩家的武器索引
		CGoods* pWeapon = ((CPlayer*)m_pMoveShape)->GetEquip( CPlayer::EQUIP_WEAPON );
		CDisplayModel* pDisplayModel = m_pMoveShape->GetDisplayModel();
		if( pWeapon && pDisplayModel)
		{
			//##0、空手;1、单手剑;2、单手锤;3、匕首;4、法杖(双手);5、双手剑(双手);6、双手斧(双手);7、弓;8、双剑(双手);9、拳刃(双手)
			long lWeaponCategory = pWeapon->GetEffectVal( GAP_WEAPON_CATEGORY, 1);
			// 根据武器索引设置光影效果
			switch(lWeaponCategory)
			{
			case WT_SINGLE_SWORD:
				pDisplayModel->EnableTrailEmitter(MAKEFOURCC('0','0','0','0'),TRUE);
				break;
			case WT_SINGLE_HAMMER:
				pDisplayModel->EnableTrailEmitter(MAKEFOURCC('0','0','0','4'),TRUE);
				break;
			case WT_SINGLE_KNIFE:
				pDisplayModel->EnableTrailEmitter(MAKEFOURCC('0','0','0','5'),TRUE);
				break;
			case WT_WAND:
				pDisplayModel->EnableTrailEmitter(MAKEFOURCC('0','0','0','3'),TRUE);
				break;
			case WT_HEAVY_SWORD:
				pDisplayModel->EnableTrailEmitter(MAKEFOURCC('0','0','0','2'),TRUE);
				break;
			case WT_HEAVY_AX:
				pDisplayModel->EnableTrailEmitter(MAKEFOURCC('0','0','0','3'),TRUE);
				break;
			case WT_DOUBLE_SWORD:
				pDisplayModel->EnableTrailEmitter(MAKEFOURCC('0','0','0','0'),TRUE);
				pDisplayModel->EnableTrailEmitter(MAKEFOURCC('0','0','0','1'),TRUE);
				break;
			case WT_FIST:
				pDisplayModel->EnableTrailEmitter(MAKEFOURCC('0','0','0','6'),TRUE);
				pDisplayModel->EnableTrailEmitter(MAKEFOURCC('0','0','0','7'),TRUE);
				break;
			default:
				pDisplayModel->SetAllTrailEmitter(FALSE);
				break;
			}
		}
	}
}


// 改变指定的属性
void CSkillXml::ChangeProperty(long lvalue, const string& strValue)
{
	if(!strValue.empty())
	{
		CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(m_lSourType, m_SourID));
		if(pMoveShape)
		{
			pMoveShape->SetProperty(strValue.c_str(),lvalue);
		}
	}
}

// 设置当前动作的规定时间
void CSkillXml::SetActTime(DWORD dwTime)
{
	// 只对本玩家和自己的宠物有用
	if (dwTime > 0 && (m_pMoveShape == GetGame()->GetMainPlayer() || (m_pMoveShape ->GetType() == TYPE_PET && ((CPet*)m_pMoveShape)->IsMainPlayerPet())))
	{
		m_pMoveShape->SetCurSkillActTime(dwTime);
		m_pMoveShape->SetCurSkillStartTime(timeGetTime());
		if (m_pMoveShape ->GetType() == TYPE_PET)
		{
			CPet *pPet = (CPet*)m_pMoveShape;
			if (pPet->IsMainPlayerPet())
			{
				tagPetSkillInfo *pPetSkillInfo = pPet->GetPetSkillInfoByID(m_lSkillID);
				if(pPetSkillInfo == NULL)	return;
				pPetSkillInfo->lSkillStartTime = GetCurTickCount();
				pPetSkillInfo->lSkillActTime = dwTime;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 执行技能结束步骤
//////////////////////////////////////////////////////////////////////////
bool CSkillXml::StepEnd(long lID, long lLevel, long sType, const CGUID& sID, float fDir)
{
	m_lStep = 2;
	m_pMoveShape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(sType, sID));
	m_lSourType = sType;
	m_SourID = sID;
	m_fDir = fDir;
	m_lSkillID = lID;
	m_lSkillLevel = lLevel;
	if (m_pMoveShape)
		m_pMoveShape->SetDirEx(m_fDir);
	
	CSkillListXml::tagSkill *pSkill = CSkillListXml::GetProperty(lID);
	if(pSkill == NULL)
		return false;
	DWORD dwlev = (long)pSkill->vectorLevels.size() >= lLevel? (DWORD)lLevel-1:((DWORD)pSkill->vectorLevels.size() - 1);
	vector<CSkillListXml::tagLevelValue> vectEnd = pSkill->vectorLevels[dwlev].vecStepEnd;
	vector<CSkillListXml::tagLevelValue>::iterator it;
	for (it = vectEnd.begin(); it != vectEnd.end(); it++)
	{
		ExecuteStep(it->dwID, it->dwValue, it->strValue);
	}
	if(m_pSkillAI)
		m_pSkillAI->StepEndAI();
	//关闭呤唱声音效果
	char strbuf[MAX_PATH]="";
	sprintf(strbuf,	"sounds/skills/%03d.wav", m_lIntonateSound);
	if(GetGame()->GetAudioList()->IsPlaying(strbuf,m_IntonateSoundChannel))
	{
		GetGame()->GetAudioList()->Stop3DSound(strbuf,m_IntonateSoundChannel);
	}
	// 删除所有对象的所有特效
	DelAllEffectListByStep(STEP_START);
	DelAllEffectListByStep(STEP_RUN);
	DelAllEffectListByStep(STEP_END);
	return true;
}

bool CSkillXml::StepSummon()
{
	return true;
}

// 设定删除特效
void CSkillXml::DelLocEffect(long l)
{
	m_lDelLocEffect = l;
}
// 设定删除索引
void CSkillXml::DelLocEffectIndex(long l)
{
	m_lDelLocEffectIndex = l;
}
// 开始删除特效
void CSkillXml::StartDelEffect(long l)
{
	CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(GetGame()->GetRegion()->FindChildObject(m_lSourType, m_SourID));
	if(pMoveShape)
		pMoveShape->DelEffect(m_lDelLocEffect, m_lDelLocEffectIndex);
}


// 取得当前技能中指定对象的特效列表
list<CSkillXml::tagEffectIndex> *CSkillXml::GetObjectEffectList(long lStep, long lType, CGUID guid)
{
	if(lStep<STEP_START || lStep >= STEP_NUM)
		return NULL;
	for(size_t i=0; i<m_vecEffectIndex[lStep].size(); ++i)
	{
		if((m_vecEffectIndex[lStep][i]).lType == lType && (m_vecEffectIndex[lStep][i]).guid == guid)
			return &((m_vecEffectIndex[lStep][i]).lsEffectIndex);
	}
	return NULL;
}


// 取得当前技能中，指定阶段的所有特效对象列表
vector<CSkillXml::tagObjectEffect> *CSkillXml::GetEffectIndex(long lStep)
{
	if(lStep<STEP_START || lStep >= STEP_NUM)
		return NULL;
	return &m_vecEffectIndex[lStep];
}


// 删除当前技能中，特定阶段的指定对象的特效列表
bool CSkillXml::DelObjectEffect(long lStep, long lTypeObj, CGUID guidObj)
{
	if(lStep < STEP_START || lStep >= STEP_NUM)
		return false;
	vector<tagObjectEffect>::iterator it = m_vecEffectIndex[lStep].begin();
	for(; it!=m_vecEffectIndex[lStep].end(); ++it)
	{
		if(it->lType == lTypeObj && it->guid == guidObj)
		{
			m_vecEffectIndex[lStep].erase(it);
			return true;
		}
	}
	return false;
}

// 删除当前技能，在指定阶段的制定对象的所有特效
bool CSkillXml::DelAllEffectListByObject(long lStep, long lTypeObj, CGUID guidObj)
{
	if(lStep < STEP_START || lStep >= STEP_NUM)
		return false;
	list<CSkillXml::tagEffectIndex> * lsEffectIndex = GetObjectEffectList(lStep, lTypeObj, guidObj);
	if(lsEffectIndex)
	{
		// 找到指定的对象
		CBaseObject * pObj = NULL;
		if(lTypeObj == TYPE_REGION)
			pObj = GetGame()->GetRegion();
		else
			pObj = GetGame()->GetRegion()->FindChildObject(lTypeObj,guidObj);
		if(pObj)
		{
			//如果该对象存在，删除先前保存的与之相关的所有特效
			list<CSkillXml::tagEffectIndex>::iterator it = lsEffectIndex->begin();
			for(; it != lsEffectIndex->end(); ++it)
			{
				CEffect *pEffect = NULL;
				// 场景
				if(pObj->GetType() == TYPE_REGION)
				{
					// 如果该特效是循环播放的，马上删除，否则让它在场景中的AI里删除
					pEffect = (CEffect *)pObj->FindChildObject(TYPE_EFFECT,(*it).guidEffect);
					if(pEffect && pEffect->IsLoop())
						pEffect->SetIsDeleted(true);
						//pObj->DeleteChildObject(TYPE_EFFECT,(*it).guidEffect);
				}
				// 玩家或者Monster或者宠物
				else if(pObj->GetType() == TYPE_PLAYER || pObj->GetType() == TYPE_MONSTER || pObj->GetType() == TYPE_PET)
				{
					// 如果该特效是循环播放的，马上删除，否则让它在自己的AI中删除
					pEffect = ((CShape *)pObj)->FindEffectByIndex((*it).guidEffect);
					if(pEffect && pEffect->IsLoop())
					{
						//pEffect->SetIsNeedAlphaFade(true);
						//pEffect->SetAlphaFadeInTimeLength(500);
						pEffect->SetIsDeleted(true);
						//((CShape *)pObj)->DelEffectByIndex((*it).guidEffect);
					}
				}
			}
		}
		lsEffectIndex->clear();
		DelObjectEffect(lStep, lTypeObj, guidObj);
		return true;
	}
	return false;
}

// 删除当前技能，在指定阶段的所有对象的所有特效
bool CSkillXml::DelAllEffectListByStep(long lStep)
{
	if(lStep < STEP_START || lStep >= STEP_NUM)
		return false;
	long lTypeObj = 0;
	CGUID guidObj = CGUID::GUID_INVALID;
	while(m_vecEffectIndex[lStep].size()>0)
	{
		lTypeObj = (m_vecEffectIndex[lStep][0]).lType;
		guidObj = (m_vecEffectIndex[lStep][0]).guid;
		if(lTypeObj != 0 && guidObj != CGUID::GUID_INVALID)
			DelAllEffectListByObject(lStep,lTypeObj,guidObj);
	}
	/*for(int i=0; i<m_vecEffectIndex[lStep].size(); ++i)
	{
		lTypeObj = (m_vecEffectIndex[lStep][i]).lType;
		guidObj = (m_vecEffectIndex[lStep][i]).guid;
		if(lTypeObj != 0 && guidObj != CGUID::GUID_INVALID)
			DelAllEffectListByObject(lStep,lTypeObj,guidObj);
	}*/
	return true;
}

// 加入一个飞行音效
void CSkillXml::SetFlySoundChannel(int nChannel)
{
	m_VecFlySoundChannel.push_back(nChannel);
}

// 关闭飞行音效
void CSkillXml::StopFlySound()
{
	for(size_t i=0;i<m_VecFlySoundChannel.size();i++)
	{
		int nChannel = m_VecFlySoundChannel[i];
		if(nChannel!=-1)
          GetGame()->GetAudioList()->Stop3DSound(nChannel);
	}
    m_VecFlySoundChannel.clear();
}

CPlayer *CSkillXml::ConvertFromPetOrPlayer(CMoveShape *pTransform)		//根据类型转换 指针
{
	if(pTransform)
	{
		CPlayer* pDestPlayer = 0;
		if(pTransform->GetType() == TYPE_PLAYER)
			pDestPlayer = (CPlayer*)pTransform;
		else if(pTransform->GetType() == TYPE_PET)
			pDestPlayer = ((CPet*)pTransform)->GetMaster();

		return		pDestPlayer;
	}
	return NULL;
}

CPlayer *CSkillXml::ConvertFromPetOrPlayer(CShape *pTransform)		//根据类型转换 指针
{
	if(pTransform)
	{
		CPlayer* pDestPlayer = 0;
		if(pTransform->GetType() == TYPE_PLAYER)
			pDestPlayer = (CPlayer*)pTransform;
		else if(pTransform->GetType() == TYPE_PET)
			pDestPlayer = ((CPet*)pTransform)->GetMaster();

		return		pDestPlayer;
	}
	return NULL;
}


/***********************************************************************/
/*zhaohang add 技能装饰实例类 */
ConcreteDecSkillXml::ConcreteDecSkillXml(SkillComponent* com):
CSkillXml(com)
{
}
ConcreteDecSkillXml::~ConcreteDecSkillXml()
{

}
/***********************************************************************/
